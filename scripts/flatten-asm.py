#!/usr/bin/env python3

from flatten.arguments import parse_arguments
from flatten.formatter import OutputFormatter
from flatten.macro import MacroInliner
from flatten.prepender import HeaderPrepender
from flatten.processor import ASMProcessor
from flatten.reorganizer import ASMReorganizer


def main() -> None:
    config = parse_arguments()

    processor = ASMProcessor(config.project_directory, config.framework_directory)
    result = processor.process_file(config.input_file)

    inliner = MacroInliner(config.macros_to_inline)
    inlined = inliner.process_lines(result)

    reorganizer = ASMReorganizer()
    reorganized = reorganizer.reorganize(inlined)

    formatter = OutputFormatter(reorganized, config.optional_labels)
    formatted = formatter.format()

    prepender = HeaderPrepender(config.header_file)
    final = prepender.prepend(formatted)

    config.output_file.write_text("\n".join(final) + "\n")
    print(f"Generated {config.output_file} ({len(final)} lines)")


if __name__ == "__main__":
    main()
