---
layout: post
date:   2016-10-31 15:22:16
title: "Using a computer to solve wordsearch"
categories: c++
---

For the last year or two I used this question for interviews I did at work, along with others. I've got bored of it and have decided it is being retired.

> I would like you to tell me how to solve a wordsearch using a computer. A wordsearch is a grid of letters that is completely filled in, plus a dictionary of words. The words may appear in the grid either left-right, right-left, top-bottom or bottom-top. A solution to the problem prints out what locations in the grid the words appear and in what direction. 

i.e.

$$
\begin{array}{|c|c|c|} 
\hline
a & d & o & g\\
\hline
e & f & g & c \\
\hline
i & j & k & a \\
\hline
m & b & a & t \\
\hline
\end{array}
$$

plus 

- dog
- cat
- ball
- bat
- ...
- 

The point of the question was to determine if candidates can at a basic level:

- clarify what the problem is,
- describe coherently how to do a procedure school children can do easily,
- discuss data structures and algorithms,
- ideally find an efficient algorithm, and
- possibly write code on the board.

If candidates cannot describe how to solve the problem in the most basic way, that is a red flag to me that they may not be able to express themselves properly. However the problem also has some really elegant solutions so that a good candidate can really show off. 

Since I'm not using it any more I thought I'd just test myself to write down the solution I'd use, which might be interesting to somebody.

# Observation 1

There is little point in solving the problem for each direction, might as well find an algorithm that works for only left-right and get the remaining directions by applying the same algorithm to the transpose, the grid reflected in the x-axis and the grid reflected in the y-axis. Now we can forget about the directions unless there is a special algorithm that does all directions efficiently.

# Bad first steps

The most common bad first step I've heard is to put the words into a hashset. The problem there is that you have committed to a dead end solution that involves checking every substring in each row, since you can only look up a whole substring in a hash table and not look at the first few characters and figure out that no words exist.

E.g. The first row is

$$
\begin{array}{|c|c|c|} 
\hline
x & a & o & g\\
\hline
\end{array}
$$

If you have a hashset {dog, cat} then clearly nothing beginning with x can possibly be a match, but you need to try out x, xa, xao, xaog because the hashset doesn't let you check.

The best algorithm starting out that way is expected $$O(d + mn^2)$$ for an m by n table with dictionary size d, because you first need to build the dictionary in time linear in the dictionary lengthXXXXXXX.

# School solutions

Describe the basic dumb algorithm

Give time complexity

# Good extensions to the basic algorithm

I like people to talk about sorting the dictionary in some way, because it basically shows that they're trying to find an efficient way to see if a particular position could be part of a word. Typically people put the words into a hashmap keyed by first letter

# Extending that

Extend that algorithm by hashing at every level

# Best solution I know

Prefix trie
