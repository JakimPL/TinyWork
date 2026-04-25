from enum import Enum
from typing import Final, Tuple

DEFAULT_MACROS_TO_INLINE: Final[Tuple[str, ...]] = ("PALETTE_OUT",)
DEFAULT_OPTIONAL_LABELS: Final[Tuple[str, ...]] = ("initialize", "set_palette")

INCLUDE: Final[str] = "%include"
IFDEF: Final[str] = "%ifdef"
IFNDEF: Final[str] = "%ifndef"
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
