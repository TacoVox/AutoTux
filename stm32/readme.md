# STM32
The STM32 code for the AutoTux project.

## Coding guidelines
- Make sure all (non-static) function names start with the file name where applicable.
- Make sure all (non-static) macro/constant names start with the file names or other clear indicator of what they concern.
- Make sure there are NO global variables that concern only one function - if so, use a static local variable instead!
- If a variable needs to be global in the file, make it <b>static</b> and it will be inaccessible to other files.
- Similarly, any function that shouldn't be accessible outside the module should be defined as static! 
- Use the specific variable you need, such as <b>uint32_t</b> or <b>uint8_t</b>, instead of the generic ones.
- Use <b>const</b> for all parameters in functions that we don't need to change in the function. Note: there are several ways to use const depending on if arguments are pointers etc.
- Consider using <b>volatile</b> on variables that are updated by interrupt routines/callbacks etc.
- Follow the comment style you see in the files: 
  -- Have a top comment in each file
  -- Have separators for different sections of the file
  -- Use javadoc style short descriptions of variables and functions. If you have more than one line, write a short description on the first line tagged with @breif. Then have an ampty line and after that the more detailed description.
  -- Don't put comments above prototypes for static functions.
  -- Avoid inline comments on variables but if you are going to use them, use the format "//!< comment".

## How to build and flash
- Compiler: gcc-arm-eabi-none should be available in your favourite packet manager
- The make_and_flash script needs st-flash, part of st-link to flash the STM: https://github.com/texane/stlink
- You may need some other library like libnewlib - just google any error messages that gets printed when running the makefile and you'll quickly find out what you're missing. 

## Licence notes
Based on the USB CDC example from ChibiOS. The ChibiOS files are licenced under the Apache licence, as stated in each relevant file (the files from ChibiOS are in the conf/ and lib/Chibios directories). 

This is stated by ChibiOS regarding the STM demos:
"Some files used by the demo are not part of ChibiOS/RT but are copyright of
ST Microelectronics and are licensed under a different license.
Also note that not all the files present in the ST library are distributed
with ChibiOS/RT, you can find the whole library on the ST web site:
http://www.st.com"
