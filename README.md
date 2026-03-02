# CS-300-Analysis-and-Design

What was the problem you were solving in the projects for this course?

ABCU needed a way to search and sort their class enrollment information. I built a small course planner that reads a CSV of courses and prerequisites, validates the data, and provides a menu to print a sorted course list or show a single course with its prereqs. The core problem was turning a flat CSV into two useful structures: a registry for fast lookup and a binary search tree for sorted printing, while keeping the program usable from the command line and meeting the project constraints.


How did you approach the problem? Consider why data structures are important to understand.

I approached the problem by breaking it into clear steps. I wrote small helper functions for trimming and tokenizing CSV lines, normalized course numbers to uppercase, and did a two pass load where the first pass builds the registry and the tree and the second pass validates prerequisites. I used std::map for the registry and a simple BST with unique_ptr for ordering, and I kept parsing and printing logic separate so each piece is easy to test and reason about. Learning when to rely on standard containers versus custom structures helped the design.


How did you overcome any roadblocks you encountered while going through the activities or project?

I ran into a few roadblocks and fixed them with focused debugging. File open failures turned out to be working directory and path issues, so printing the current path and showing a sensible default filename helped a lot. Parsing bugs came from using the wrong tokenizer, so I replaced it with a simpler splitter. Input problems came from mixing operator>> and getline, so I either flushed the input correctly with numeric_limits or switched to getline for menu input. Enabling or removing filesystem usage solved C++ standard issues in some environments.


How has your work on this project expanded your approach to designing software and developing programs?

Working on this project changed how I design small programs. I now think more about input validation and user experience up front, and I favor clear interfaces between parsing, validation, and presentation. I plan features so that data flows through well defined steps, which makes it easier to add improvements later and keeps responsibilities separate for testing and debugging.


How has your work on this project evolved the way you write programs that are maintainable, readable, and adaptable?

My code style has become more deliberate and maintainable. I write small reusable helpers, name things consistently, add short comments where behavior could be surprising, and use RAII and smart pointers to avoid manual memory management. I also try to make error messages and prompts friendly so a grader or user can reproduce and fix problems quickly. These habits make the code easier to read, adapt, and extend.
