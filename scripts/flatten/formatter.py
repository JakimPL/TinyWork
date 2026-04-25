from typing import List, Set, Tuple

from flatten.constants import GLOBAL


class OutputFormatter:
    def __init__(self, lines: List[str], optional_labels: Tuple[str, ...] = ()) -> None:
        self.lines = lines
        self.optional_labels = tuple(f"{label}:" for label in optional_labels)

    def should_keep_line(self, line: str) -> bool:
        stripped = line.strip()
        if not stripped:
            return True

        first_word = stripped.split()[0] if stripped.split() else ""
        return first_word != GLOBAL

    def is_label(self, line: str) -> bool:
        stripped = line.strip()
        return stripped.endswith(":")

    def is_empty_line(self, line: str) -> bool:
        return not line.strip()

    def is_double_underscore_label(self, line: str) -> bool:
        stripped = line.strip()
        return stripped.startswith("__") and stripped.endswith(":")

    def has_code_before_next_label(self, lines: List[str], start_index: int) -> bool:
        for i in range(start_index + 1, len(lines)):
            line = lines[i].strip()
            if not line:
                continue

            if self.is_label(lines[i]):
                return False

            if line.startswith("section"):
                return False

            return True

        return False

    def is_empty_optional_label(self, lines: List[str], index: int) -> bool:
        line = lines[index].strip()
        if line not in self.optional_labels:
            return False
        return not self.has_code_before_next_label(lines, index)

    def filter_unwanted_labels(self) -> List[str]:
        filtered = []
        for line in self.lines:
            if not self.should_keep_line(line):
                continue

            if self.is_double_underscore_label(line):
                continue

            filtered.append(line)

        return filtered

    def find_empty_optional_labels(self, lines: List[str]) -> Set[int]:
        indices_to_remove: Set[int] = set()
        for i, line in enumerate(lines):
            if self.is_empty_optional_label(lines, i):
                indices_to_remove.add(i)
        return indices_to_remove

    def remove_redundant_empty_lines(self, lines: List[str], skip_indices: Set[int]) -> List[str]:
        formatted = []
        previous_was_label = False
        previous_was_empty = False

        for i, line in enumerate(lines):
            if i in skip_indices:
                continue

            is_empty = self.is_empty_line(line)

            if is_empty and previous_was_label:
                previous_was_empty = True
                continue

            if is_empty and previous_was_empty:
                continue

            formatted.append(line)
            previous_was_label = self.is_label(line)
            previous_was_empty = is_empty

        return formatted

    def format(self) -> List[str]:
        filtered = self.filter_unwanted_labels()
        empty_labels = self.find_empty_optional_labels(filtered)
        formatted = self.remove_redundant_empty_lines(filtered, empty_labels)
        return formatted
