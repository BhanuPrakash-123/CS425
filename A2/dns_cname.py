import dns.message
import dns.query
import dns.rdatatype
import dns.resolver
import time

# Root DNS servers used to start the iterative resolution process
ROOT_SERVERS = {
    "198.41.0.4": "Root (a.root-servers.net)",
    "199.9.14.201": "Root (b.root-servers.net)",
    "192.33.4.12": "Root (c.root-servers.net)",
    "199.7.91.13": "Root (d.root-servers.net)",
    "192.203.230.10": "Root (e.root-servers.net)"
}

# Timeout in seconds for each DNS query attempt
TIMEOUT = 3

def send_dns_query(server, domain, rdatatype=dns.rdatatype.A):
    """
    Sends a DNS query to the specified server for the given domain and record type.
    If the query is successful, it returns the response. Otherwise, it returns None.
    """
    try:
        # Construct the DNS query for the specified domain and record type
        query = dns.message.make_query(domain, rdatatype)
        # Send the query to the DNS server using UDP and wait for the response
        response = dns.query.udp(query, server, timeout=TIMEOUT)
        return response  # Return the response received from the server
    except Exception:
        return None  # If an error occurs (e.g., timeout, unreachable server), return None

def extract_next_nameservers(response):
    """
    Extracts nameserver (NS) records from the authority section of the DNS response.
    Resolves those NS names to IP addresses and returns a list of the resolved IPs.
    """
    ns_ips = []  # List to store resolved nameserver IPs
    ns_names = []  # List to store nameserver domain names

    # Loop through the authority section of the response to extract NS records
    for rrset in response.authority:
        if rrset.rdtype == dns.rdatatype.NS:
            for rr in rrset:
                ns_name = rr.to_text()  # Extract the nameserver hostname
                ns_names.append(ns_name)
                print(f"Extracted NS hostname: {ns_name}")

    # Resolve the extracted NS hostnames to IP addresses
    for ns_name in ns_names:
        try:
            # Perform an A record query for the nameserver to get its IP address
            answer = dns.resolver.resolve(ns_name, "A")
            for rdata in answer:
                ns_ips.append(rdata.address)  # Store the resolved IP address
                print(f"Resolved {ns_name} to {rdata.address}")  # Debug output
        except dns.resolver.NXDOMAIN:
            print(f"[ERROR] No such domain: {ns_name}")  # If the NS domain does not exist
        except dns.exception.Timeout:
            print(f"[ERROR] Timeout resolving {ns_name}")  # If the resolution process times out
        except dns.resolver.NoAnswer:
            print(f"[ERROR] No answer for {ns_name}")  # If no answer is returned for the A record query

    return ns_ips  # Return the list of resolved nameserver IP addresses

def iterative_dns_lookup(domain):
    """
    Performs an iterative DNS resolution starting from root servers.
    It queries root servers, then TLD servers, then authoritative servers,
    following the DNS hierarchy until an answer is found or the resolution fails.
    """
    print(f"[Iterative DNS Lookup] Resolving {domain}")

    next_ns_list = list(ROOT_SERVERS.keys())  # Start with the root server IPs
    stage = "ROOT"  # Track the current resolution stage (ROOT, TLD, AUTH)

    while next_ns_list:
        ns_ip = next_ns_list.pop(0)  # Pick the first available nameserver to query
        response = send_dns_query(ns_ip, domain)

        if response:  # Check if the response is not None
            print(f"[DEBUG] Querying {stage} server ({ns_ip}) - SUCCESS")

            # If an answer is found, print it and return
            if response.answer:
                for answer in response.answer:
                    if answer.rdtype == dns.rdatatype.CNAME:
                        cname_target = answer[0].target.to_text()
                        print(f"[INFO] {domain} is a CNAME for {cname_target}")
                        print(f"Please run the resolver again with the CNAME target: {cname_target}")
                        return
                    elif answer.rdtype == dns.rdatatype.A:
                        for item in answer.items:
                            print(f"[SUCCESS] {domain} -> {item.address}")
                        return

            # If no answer, extract the next set of nameservers
            next_ns_list = extract_next_nameservers(response)
            # Move to the next resolution stage (ROOT -> TLD -> AUTH)
            if stage == "ROOT":
                stage = "TLD"  # Transition from root servers to Top-Level Domain servers
            elif stage == "TLD":
                stage = "AUTH"  # Transition from Top-Level Domain servers to authoritative servers

        else:
            print(f"[ERROR] Query failed for {stage} {ns_ip}")

    print("[ERROR] Resolution failed.")  # Final failure message if no nameservers respond

def recursive_dns_lookup(domain):
    """
    Performs recursive DNS resolution using the system's default resolver.
    This approach relies on a resolver (like Google DNS or a local ISP resolver)
    to fetch the result recursively.
    """
    print(f"[Recursive DNS Lookup] Resolving {domain}")
    try:
        # First, check if the domain has a CNAME record
        try:
            cname_answer = dns.resolver.resolve(domain, "CNAME")
            for rdata in cname_answer:
                cname_target = rdata.target.to_text()
                print(f"[INFO] {domain} is a CNAME for {cname_target}")
                domain = cname_target  # Update domain to resolve the CNAME target
        except dns.resolver.NoAnswer:
            pass  # No CNAME record, continue with A and NS resolution

        # Resolve NS (Name Server) records for the domain
        try:
            answer = dns.resolver.resolve(domain, "NS")
            for rdata in answer:
                print(f"[SUCCESS] {domain} -> {rdata}")  # Print each nameserver
        except dns.resolver.NoAnswer:
            print(f"[INFO] No NS records found for {domain}")

        # Resolve A (IPv4 Address) records for the domain
        answer = dns.resolver.resolve(domain, "A")
        for rdata in answer:
            print(f"[SUCCESS] {domain} -> {rdata}")  # Print the resolved IP address

    except dns.resolver.NXDOMAIN:
        print(f"[ERROR] Domain {domain} does not exist")
    except Exception as e:
        print(f"[ERROR] Recursive lookup failed: {e}")

if __name__ == "__main__":
    import sys
    if len(sys.argv) != 3 or sys.argv[1] not in {"iterative", "recursive"}:
        print("Usage: python3 dns_server.py <iterative|recursive> <domain>")
        sys.exit(1)

    mode = sys.argv[1]  # Get the mode (iterative or recursive)
    domain = sys.argv[2]  # Get the domain to resolve
    start_time = time.time()  # Record the start time

    # Execute the selected DNS resolution mode
    if mode == "iterative":
        iterative_dns_lookup(domain)
    else:
        recursive_dns_lookup(domain)

    print(f"Time taken: {time.time() - start_time:.3f} seconds")  # Print the execution time