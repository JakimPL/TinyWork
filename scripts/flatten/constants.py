from enum import Enum
from typing import Final, Tuple

DEFAULT_MACROS_TO_INLINE: Final[Tuple[str, ...]] = ("PALETTE_OUT",)

INCLUDE: Final[str] = "%include"
IFDEF: Final[str] = "%ifdef"
IFNDEF: Final[str] = "%ifndef"
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
