Booth Simulation

This project simulates multiple booths servicing customers from different queues. It utilizes a round-robin approach to ensure fair service distribution among customers.

Features

Creation and management of multiple queues
Dynamic allocation and deallocation of memory for customers and queues
Simulation of booth operations based on customer arrival times and checkout durations
Fair distribution of customers among booths using a round-robin approach
Usage

Upon running the executable, you will be prompted to input the number of operations and the number of booths for the simulation. Follow the on-screen instructions to provide the necessary inputs.

The simulation will then proceed to process customers based on their arrival times and checkout durations, printing the checkout information for each customer.

Input Files
The input files follow the naming convention movielineXX.in, where XX ranges from 01 to 10. Each input file represents a different scenario with varying numbers of customers and booths.


Compile the source code:
gcc movieline.c -o movieline

Run the compiled executable with an input file:
./movieline < movieline01.in

Replace movieline01.in with the desired input file (e.g., movieline02.in, movieline03.in, etc.).

Follow the prompts to provide additional inputs as required by the simulation.
The simulation will process the input and display the checkout information for each customer, simulating the movie ticket purchasing process.
Sample Input Files
Sample input files (movieline01.in to movieline10.in) are provided in the repository to demonstrate various scenarios and test the simulation.


Contributing

Contributions are welcome! If you find any issues or have suggestions for improvement, please feel free to open an issue or submit a pull request.






