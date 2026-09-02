from pathlib import Path

from flatten.constants import DEFINE, ELIF, ELSE, ENDIF, IF, IFDEF, IFNDEF, INCLUDE, Directive
from flatten.preprocessor import GuardTracker, PreprocessorState


class ASMProcessor:
    def __init__(
        self,
        source_path: Path,
        framework_path: Path,
        *,
        defines: frozenset[str],
    ) -> None:
        self.source_path = source_path
        self.framework_path = framework_path
        self.state = PreprocessorState(defines)
        self.guards = GuardTracker()
        self.conditionals: list[bool] = []

    def resolve_path(self, include_path: str) -> Path:
        if include_path.startswith("tiny/"):
            relative_path = include_path.removeprefix("tiny/")
            return self.framework_path / "tiny" / relative_path
        else:
            return self.source_path / include_path

    def get_directive(self, line: str) -> str:
        parts = line.split()
        return parts[0] if parts else ""

    def get_word(self, line: str, index: int, default: str = "") -> str:
        parts = line.split()
        return parts[index] if len(parts) > index else default

    def process_file(self, file_path: Path) -> list[str]:
        lines = file_path.read_text().splitlines()
        result = []

        for line in lines:
            processed = self.process_line(line)
            if processed is not None:
                result.extend(processed)

        return result

    def process_line(self, line: str) -> list[str] | None:
        stripped = line.strip()
        directive = self.get_directive(stripped)

        if self.guards.is_skipping():
            if directive.startswith(IF):
                self.guards.enter_ifdef()
            elif directive.startswith(ENDIF):
                self.guards.exit_conditional()
            return []

        match directive:
            case d if d.startswith(INCLUDE):
                return self.handle_include(stripped)
            case d if d.startswith(IFDEF):
                return self.handle_ifdef(stripped)
            case d if d.startswith(IFNDEF):
                return self.handle_ifndef(stripped)
            case d if d.startswith(ELIF):
                return self.handle_conditional_line(line)
            case d if d.startswith(IF):
                return self.handle_if(line)
            case d if d.startswith(DEFINE):
                return self.handle_define(line)
            case d if d.startswith(ELSE):
                return self.handle_else(line)
            case d if d.startswith(ENDIF):
                return self.handle_endif(line)
            case _:
                self.guards.break_guard_pattern()
                return [line] if self.state.should_keep_line() else []

    def handle_include(self, line: str) -> list[str]:
        if not self.state.should_keep_line():
            return []

        parts = line.split('"')
        if len(parts) < 2:
            raise ValueError(f"Malformed include directive: {line}")

        include_path = parts[1]
        full_path = self.resolve_path(include_path)
        return self.process_file(full_path)

    def handle_ifdef(self, line: str) -> list[str]:
        symbol = self.get_word(line, 1)
        self.guards.enter_ifdef()
        self.state.push_block(Directive.IFDEF, symbol)
        self.conditionals.append(False)
        return []

    def handle_ifndef(self, line: str) -> list[str]:
        symbol = self.get_word(line, 1)

        should_skip = self.guards.enter_ifndef(symbol)
        if should_skip:
            return []

        self.state.push_block(Directive.IFNDEF, symbol)
        self.conditionals.append(False)
        return []

    def handle_if(self, line: str) -> list[str]:
        """A test the assembler answers for itself, such as %if or %ifmacro.

        The flattener knows nothing of the values these ask about, so the whole block goes
        through as it stands, %endif included, and only the guards around includes are
        resolved here.
        """
        self.guards.enter_ifdef()
        self.conditionals.append(True)
        return self.handle_conditional_line(line)

    def handle_conditional_line(self, line: str) -> list[str]:
        """A line belonging to a block the assembler decides, kept exactly as written."""
        return [line] if self.state.should_keep_line() else []

    def handle_define(self, line: str) -> list[str]:
        symbol = self.get_word(line.strip(), 1)

        should_output = self.guards.process_define(symbol)
        if not should_output:
            return []

        return [line] if self.state.should_keep_line() else []

    def handle_else(self, line: str) -> list[str]:
        if self.in_conditional():
            return self.handle_conditional_line(line)

        self.guards.handle_else()
        self.state.handle_else()
        return []

    def handle_endif(self, line: str) -> list[str]:
        self.guards.exit_conditional()
        if self.conditionals.pop() if self.conditionals else False:
            return self.handle_conditional_line(line)

        self.state.pop_block()
        return []

    def in_conditional(self) -> bool:
        """Whether the innermost block open here is one the assembler decides."""
        return bool(self.conditionals) and self.conditionals[-1]
