from enum import Enum
from typing import Final

DEFAULT_DEFINES: Final[tuple[str, ...]] = ("COM", "DOS")
DEFAULT_MACROS_TO_INLINE: Final[tuple[str, ...]] = ("PALETTE_OUT",)
DEFAULT_OPTIONAL_LABELS: Final[tuple[str, ...]] = ("initialize", "set_palette")

INCLUDE: Final[str] = "%include"
IF: Final[str] = "%if"
IFDEF: Final[str] = "%ifdef"
IFNDEF: Final[str] = "%ifndef"
ELIF: Final[str] = "%elif"
DEFINE: Final[str] = "%define"
ELSE: Final[str] = "%else"
ENDIF: Final[str] = "%endif"
MACRO: Final[str] = "%macro"
ENDMACRO: Final[str] = "%endmacro"
GLOBAL: Final[str] = "global"


class Directive(Enum):
    IFDEF = "ifdef"
    IFNDEF = "ifndef"
    ELSE = "else"
    ENDIF = "endif"
