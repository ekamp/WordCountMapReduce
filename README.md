WordCountMapReduce
==================

OS Programming Assignment 2, Create a map reduce program to count the number of words or numbers in a file.

- In order to do this properly we need to utilize a hashmap to store the word as a key and an occurance as a value.
- Each mapper will get that key value pair and the reducer will combine the results of the various mappers
- For the input, we need to split the files by N parts or N mappers so that the N mappers can work with its assigned part
  - In order to do this wee need to use the split command explained in the documentation in order to split the file into N parts
  - Once split we assign the parts to the various workers

