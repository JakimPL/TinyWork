from dataclasses import dataclass
from pathlib import Path


@dataclass
class FlattenConfig:
    project_directory: Path
    framework_directory: Path
    source_directory: Path
    input_file: Path
    output_file: Path
    macros_to_inline: tuple[str, ...]
    optional_labels: tuple[str, ...]
    defines: tuple[str, ...]

    @property
    def header_file(self) -> Path:
        return self.source_directory / "info.asm"
