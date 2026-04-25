from dataclasses import dataclass
from pathlib import Path
from typing import Tuple


@dataclass
class FlattenConfig:
    project_directory: Path
    framework_directory: Path
    input_file: Path
    output_file: Path
    macros_to_inline: Tuple[str, ...]
    optional_labels: Tuple[str, ...]

    @property
    def header_file(self) -> Path:
        return self.project_directory / "core" / "info.asm"
