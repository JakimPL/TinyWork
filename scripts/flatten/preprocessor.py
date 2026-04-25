from typing import List, Optional, Set

from flatten.constants import Directive


class GuardTracker:
    def __init__(self) -> None:
        self._seen_guards: Set[str] = set()
        self._expecting_guard_define: Optional[str] = None
        self._skip_depth = 0
        self._current_depth = 0

    def is_skipping(self) -> bool:
        return self._skip_depth > 0

    def enter_ifndef(self, symbol: str) -> bool:
        self._current_depth += 1

        if symbol in self._seen_guards:
            self._skip_depth = self._current_depth
            return True

        self._expecting_guard_define = symbol
        return False

    def enter_ifdef(self) -> None:
        self._current_depth += 1
        self._expecting_guard_define = None

    def exit_conditional(self) -> None:
        if self._skip_depth == self._current_depth:
            self._skip_depth = 0

        self._current_depth -= 1
        self._expecting_guard_define = None

    def process_define(self, symbol: str) -> bool:
        if symbol == self._expecting_guard_define:
            self._seen_guards.add(symbol)
            self._expecting_guard_define = None
            return False

        self._expecting_guard_define = None
        return True

    def handle_else(self) -> None:
        self._expecting_guard_define = None

    def break_guard_pattern(self) -> None:
        self._expecting_guard_define = None


class PreprocessorBlock:
    def __init__(self, directive: Directive, symbol: str, parent_active: bool) -> None:
        self.directive = directive
        self.symbol = symbol
        self.parent_active = parent_active
        self.in_else = False
        self.was_active = self.compute_active()

    def compute_active(self) -> bool:
        if not self.parent_active:
            return False

        is_defined = self.symbol in {"COM", "DOS"}

        match self.directive:
            case Directive.IFDEF:
                return is_defined
            case Directive.IFNDEF:
                return not is_defined
            case _:
                return False

    def get_active(self) -> bool:
        if self.in_else:
            return self.parent_active and not self.was_active

        return self.was_active


class PreprocessorState:
    def __init__(self) -> None:
        self.block_stack: List[PreprocessorBlock] = []

    def should_keep_line(self) -> bool:
        for block in self.block_stack:
            if not block.get_active():
                return False

        return True

    def push_block(self, directive: Directive, symbol: str) -> None:
        parent_active = self.should_keep_line()
        block = PreprocessorBlock(directive, symbol, parent_active)
        self.block_stack.append(block)

    def handle_else(self) -> None:
        if self.block_stack:
            self.block_stack[-1].in_else = True

    def pop_block(self) -> None:
        if self.block_stack:
            self.block_stack.pop()
