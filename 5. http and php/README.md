# HTTP and PHP
*　The max78000 sends a post request that contains its name to PHP to log in.
* MAX78000 program filse are in the client folder. PHP and SQL file is in the server folder.
* I use xampp on windows to execute this code.
## Client side
* compile the code and load the code onto MAX78000 as https://github.com/MaximIntegratedAI/MaximAI_Documentation/blob/master/MAX78000_Evaluation_Kit/README.md

## Server side
* Put max.php into ..\xampp\htdocs
* Import maxserver.sql
* Apache and mysql should both be running.

# Unsolved problems
* The header of the response can still not be fully read. A huge part of the header will be missing, but the content is still usable.