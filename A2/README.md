# Assignment 2 : CS425
### Code by Aman(220413), Bhanu(220583), Rithwin(220537)

This project implements a DNS resolver in Python that can perform both iterative and recursive DNS lookups. The resolver starts from the root DNS servers and follows the DNS hierarchy to resolve domain names to their corresponding IP addresses and also provides a means for CNAME handling.

### Pre-requisites

- Python 3.x
- `dnspython` library

### Files Available
There are two python files available, one being "dns_cname.py" which contains the CNAME handling but modified base code and the other is "dnsresolver.py" which is the original code with the required part implemented only and no means for CNAME handling.


### Usage

To run this program, firstly decide whether you want to run the recursive or the iterative lookup and the command is to be run in the format of

python3 dns_server.py <iterative|recursive> <domain> where dns_server.py is the name of the python file containing the dns resolver.

For eg: python3 test.py iterative youtube.com

### Features Implemented

- **Iterative DNS Lookup**: Resolves domain names by querying root servers, TLD servers, and authoritative servers in sequence.
- **Recursive DNS Lookup**: Uses the system's default resolver to fetch the result recursively.
- **CNAME Handling**: If a CNAME record is encountered, the user is prompted to run the resolver again with the CNAME target.

### Some Loose Ends

- In case of CNAME, the user is required to enter the CNAME target to get the IP of the domain .


## Contributions

- Aman (220413) : 33.33% - README and CNAME handling
- Rithwin (220537) : 33.33% - Code implementation
- Bhanu (220583) : 33.33% - Testing and Debugging

### Challenges faced

Were unable to determine how to resolve the cases of CNAME and decided to print the CNAME target so that the user can run the target to get the final IP

### Sources referred

- dnspython Documentation
- Python `socket` Library Documentation
- Wikipedia - Domain Name System

## Declaration
We declare that we did not indulge in plagiarism and the work submitted is our own.
