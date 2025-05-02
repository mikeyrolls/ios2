# IOS project 2 (2024/25)

2BIT

?/15

*due 4.5.25*

## Task

- semaphores (pdf with requirements in `extras/`)
- `.c` and header files in root dir, `Makefile` mandatory

Ferry moving cars and trucks between 2 ports, details in pdf

## Executable

- build with make

    Usage: `./proj2 N O K TA TP`

| | | |
|-|-|-|
| N |  Number of trucks | N < 10000 |
| O |  Number of passenger cars | O < 10000 |
| K |  Ferry capacity (maximum number of passenger cars that can be loaded) | 3 <= K <= 100 |
| TA | Maximum travel time for a car to reach the port (in microseconds) | 0 <= TA <= 10000 |
| TP | Maximum travel time for the ferry (in microseconds) | 0 <= TP <= 1000 |


## Makefile

no external runner script, switching preset arguments in makefile

### Usable

make executable:

    make

delete binaries, out files and zips:

    make clean

build, run preset and clean up:

    make run

build, run preset, test output and clean up:

    make test

zip files to hand in:

    make zip

### Helper funcs

build and run preset, keeping binaries:

    make run-not-clean

## Notes

- Makefile testing
- doxygen

## Bibliography

- [Semaphores](https://ia800305.us.archive.org/12/items/semaphores/semaphores.pdf)
- [Makefile](https://makefiletutorial.com/)