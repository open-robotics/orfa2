OPEN ROBOTICS FIRMWARE ARCHITECTURE
===================================

Second generaion.

Main goal is to provide modules for hobbyst robotics.
(*TODO: write complete readme*)


Supported modules
=================

Currently only OR-STM32F1-DS Rev.1.


Build
=====

Build requires gcc cross toolchain for ARM (arm-none-eabi), ChibiOS/RT trunk.

git clone git://github.com/open-robotics/orfa2.git orfa2
cd orfa2
make -C ext
make
