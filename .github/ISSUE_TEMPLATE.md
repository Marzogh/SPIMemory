Hey there! Thanks for using the SPIFlash library for Arduino. 
### Please note that starting 01.03.2018 any issue raised here MUST be submitted according to this template or it will be flagged with 'Not enough information'. No action will be taken till all the prerequisite information is provided. If no information is provided for over a month, the issue will be closed.

**Note: For support questions, please use the [Arduino Forums](http://forum.arduino.cc/index.php?topic=324009.0). This repository's issues are reserved for feature requests and bug reports.**

# Issue details

**I'm submitting a ...**
  - [ ] bug report
  - [ ] feature request
  - [ ] support request => Please do not submit support request here, see note at the top of this template.
  
Do the checklist before filing an issue:

- [ ] Is this something you can **debug and fix**? Send a pull request! Bug fixes and documentation fixes are welcome.
- [ ] Is this an idea for a feature? Post the feature request as an issue here with a title that clearly says "Feature Request".
- [ ] Is this a bug that you cannot fix? Post the bug report as an issue here with a title that cleary says "Bug Report".
  
When opening an issue please include the following details:
-------------------------------------------------------------

- [ ] Do you want to request a *feature* or report a *bug*?
- [ ] Which library version are you using?
- [ ] Which Arduino IDE version are you compiling the library on?
- [ ] Which OS are you compiling your code on? [Linux / Mac / Windows]
- [ ] Which microcontroller platform are you using?
- [ ] Which Flash memory module are you using?
- [ ] What is the current behavior?
- [ ] What is the expected behavior?

--------------------------
###### Bug reports only
- If this is a bug report - 

- [ ] Make sure you have run FlashDiagnostics.ino with ``` #define RUNDIAGNOSTICS ``` uncommented in SPIFlash.h. **List any error codes** that pop up in your Serial output when you run FlashDiagnostics.ino.here:
      Error codes
      ------------
        - 
        -
        -
        -
        -
- [ ] If you have a problem with a particular function, call the flash.error() function (after you have made sure you have started up your Serial port with a ``` Serial.begin(BAUD) ``` ). Provide details of the function, the data given to/ expected from the function and the error code here: (**Please repeat this for every function you have an error with***)
        Function - 
        Data - 
        Error code - 
- [ ] Provide a **minimal code snippet** example that reproduces the bug. Please make sure you wrap any code in the proper code blocks like below 
```
```CODE HERE```
```
- [ ] Provide **screenshots** where appropriate
- [ ] Provide **wiring diagram** where appropriate

-----------------------------
###### Feature Requests only

- [ ] If this is a feature request, what is the motivation / use case for changing the behavior?

----------------------------

- [ ] Other information (e.g. detailed explanation, related issues, suggestions how to fix, links for us to have context etc.)

_Make sure to add **all the information needed to understand the bug/feature** so that someone can help. If the info is missing we'll add the 'Needs more information' label and close the issue until there is enough information._
