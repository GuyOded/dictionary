*The program is written quite shity, I had a lot of naming issues and some trouble procedurally-had some difficulties
with deviding the work to functions

This file contains all the functions that parse the dictionary- searching for a word, getting a synonym, initializing the 
index array letters etc. The main function is find_synonym which does all the searching and parsing.
The letters array contains the position of all the defined letters in the dictionary file. The function set_letters_pos do that
by counting characters. To navigate to a certain letters we use fseek(dictionary, letters[letter - 'A'], SEEK_SET) instead of iterating over the whole file.

The format of the dictionary file:
Example:
B(letter)
beginning(word)-1(number of synonym)|starting(synonym)|
P
peeped-1|glanced|
pictures-1|drawings|
R
rabbit-2|hare|bunny|
T
tired-1|sleepy|

-Every letter must be a capital letter.
-All words should end with a '-' without spaces what so ever.
-After the word must come a number representing the number of synonym available.
-There must be a '|' separating the number of synonyms and each synonym from each other.
-Every letter mustn't exist more once in the dictionary file.

search_and_replace.c
replace_main-The main function that goes over the text file given in the command line arguments and replaces all the words that exist with a random synonym.
We create a new file named temp.txt, that is the file with the replaced synonym(the original file is kept).

Note that the first command line argument is the dictionary file and the second is the text file.
