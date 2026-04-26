import argparse
from pathlib import Path

from flatten.config import FlattenConfig
from flatten.constants import DEFAULT_MACROS_TO_INLINE, DEFAULT_OPTIONAL_LABELS


def parse_arguments() -> FlattenConfig:
    """Parse command line arguments and return configuration."""
    parser = argparse.ArgumentParser(description="Flatten assembly code by inlining includes and macros")
    parser.add_argument(
        "--project-directory",
        type=Path,
        default=Path.cwd(),
        help="Project root directory (default: current directory)",
    )
    parser.add_argument(
        "--framework-directory",
        type=Path,
        required=True,
        help="Framework root directory (where tiny/ folder is located)",
    )
    parser.add_argument(
        "--src-directory",
        type=Path,
        required=True,
        help="Source directory for project-specific assembly files",
    )
    parser.add_argument(
        "--input",
        type=Path,
        required=True,
        help="Input assembly file (typically main.asm from framework)",
    )
    parser.add_argument(
        "--output",
        type=Path,
        required=True,
        help="Output flattened assembly filepath",
    )
    parser.add_argument(
        "--inline-macros",
        type=str,
        nargs="*",
        default=DEFAULT_MACROS_TO_INLINE,
        help=f"Macro names to inline (default: {', '.join(DEFAULT_MACROS_TO_INLINE)})",
    )
    parser.add_argument(
        "--optional-labels",
        type=str,
        nargs="*",
        default=DEFAULT_OPTIONAL_LABELS,
        help=f"Labels to remove if empty (default: {', '.join(DEFAULT_OPTIONAL_LABELS)})",
    )

    args = parser.parse_args()

    return FlattenConfig(
        project_directory=args.project_directory.resolve(),
        framework_directory=args.framework_directory.resolve(),
        source_directory=args.src_directory.resolve(),
        input_file=args.input.resolve(),
        output_file=args.output.resolve(),
        macros_to_inline=tuple(args.inline_macros),
        optional_labels=tuple(args.optional_labels),
    )
