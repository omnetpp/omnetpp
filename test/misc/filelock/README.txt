Writer continuously refills the file with a number of lines.
A single line is filled with a character of the alphabet.
Subsequent lines have the subsequent characters with wrap around.
The character for the first line is random.

Reader continuously reads the whole file at once.
Checks if subsequent lines have subsequent characters in them.
If locking is not used, then occasionally this test will fail.

File reader continuously reads lines at random positions.
A single line must be filled with the same character.
If locking is not used, then occasionally this test will fail.
