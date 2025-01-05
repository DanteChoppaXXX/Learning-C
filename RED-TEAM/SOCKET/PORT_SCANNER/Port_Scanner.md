### **Algorithm: Robust Port Scanner Program in C**

A **robust port scanner** is designed to detect open ports on a specified host or IP address by attempting connections to a range of ports and identifying their statuses. It can support scanning for both **TCP** and **UDP** ports, incorporate error handling, and optimize performance with multithreading or non-blocking sockets.

---

## **Step 1: Import Required Libraries**

- Include libraries for networking, multithreading, and input/output operations:
  - **Networking**: Socket programming support for communication.
  - **Multithreading**: For parallel scanning to improve performance.
  - **Time management**: To handle timeouts for connections.
  - **Input/output**: For displaying results and handling user input.

---

## **Step 2: Initialize Program Variables**

1. **Target IP/Hostname**:
   - Accept target IP address or hostname as input.
2. **Port Range**:
   - Accept start and end port numbers to scan.
3. **Connection Timeout**:
   - Set the timeout duration for connections (e.g., 1–2 seconds).
4. **Protocol Type**:
   - Choose between **TCP** or **UDP** scanning.
5. **Thread Pool (Optional)**:
   - Configure the number of threads for parallel scanning.

---

## **Step 3: Resolve Hostname to IP Address**

- If the input is a hostname, perform a **DNS lookup** to resolve it to an IP address.
- Validate whether the provided IP or resolved IP is valid and reachable.

---

## **Step 4: Implement Port Scanning Logic**

### **TCP Port Scanning Algorithm**

1. **Iterate Through Ports**:
   - Loop through the range of ports specified (start to end).
2. **Create a Socket**:
   - For each port, create a TCP socket.
3. **Set Timeout for Connection**:
   - Apply a short timeout to avoid long waits for unresponsive ports.
4. **Attempt to Connect**:
   - Use the `connect()` function to test if a connection is successful.
5. **Check Result**:
   - If the connection is successful, mark the port as **open**.
   - If the connection fails, mark it as **closed** or **filtered**.
6. **Close Socket**:
   - Close the socket after testing each port.

---

### **UDP Port Scanning Algorithm**

1. **Create UDP Socket**:
   - Use UDP sockets instead of TCP sockets.
2. **Send Dummy Data**:
   - Send a small data packet to each port.
3. **Wait for Response**:
   - Use `recvfrom()` to wait for an ICMP "Port Unreachable" response.
4. **Evaluate Response**:
   - If no response is received, assume the port might be **open** or **filtered**.
   - If an error response is received, mark the port as **closed**.
5. **Close Socket**:
   - Close the UDP socket for each port.

---

## **Step 5: Display Results**

1. For each scanned port:
   - Print **Open**, **Closed**, or **Filtered** status.
2. Provide summary statistics:
   - Total ports scanned.
   - Number of open, closed, and filtered ports.

---

## **Step 6: Optimize Scanning with Multithreading (Optional)**

### **Multithreaded Implementation (Pseudo Code)**

1. Create a **thread pool**:
   - Divide the ports to scan among multiple threads.
2. Assign a **worker function** to each thread:
   - Each thread processes a subset of ports concurrently.
3. Synchronize output:
   - Use locks or mutexes to prevent mixed outputs from multiple threads.

**Example Pseudo Code**:

```
Thread_Pool pool;
initialize_thread_pool(pool, MAX_THREADS);

for each port in range:
    assign thread from pool to scan_port(port);

wait_for_all_threads_to_finish(pool);
```

---

## **Step 7: Handle Errors and Exceptions**

1. Invalid Target IP or Hostname:
   - Display error and exit the program.
2. Port Range Validation:
   - Ensure the start port is less than or equal to the end port and both are within valid ranges (0–65535).
3. Socket Creation Failure:
   - Retry or display error for specific ports.
4. Connection Timeout:
   - Skip the port after timeout and log the status as **filtered**.
5. Thread Failures:
   - Safely terminate threads and clean up resources.

---

## **Step 8: Additional Features (Optional Enhancements)**

1. **Service Detection**:

   - Implement basic banner grabbing by reading the first response message from open ports to identify the service (e.g., HTTP, FTP).

   ```
   if port is open:
       send empty request or specific data packet;
       receive response and analyze service type.
   ```

2. **Port Filtering Detection**:

   - Check if ports are filtered (firewall blocks) by examining timeouts or connection resets.

3. **Scan Multiple Targets**:

   - Add support to scan multiple IPs or subnets using CIDR notation.

4. **Export Results**:

   - Provide an option to save results in a file (e.g., CSV or JSON format).

5. **Timeout Control**:

   - Allow users to specify timeouts for faster scanning or higher accuracy.

6. **IPv6 Support**:
   - Extend functionality to support IPv6 scanning.

---

## **Step 9: Final Cleanup and Exit**

1. Close all open sockets.
2. Free dynamically allocated memory, if any.
3. Print a summary of the scan, including time taken and number of ports scanned.
4. Exit the program gracefully.

---

## **Pseudo Code Example**

```
START
    INPUT target_IP, start_port, end_port, timeout, protocol_type

    VALIDATE target_IP and port range
    IF invalid INPUT THEN
        PRINT error and EXIT

    RESOLVE hostname to IP, if necessary

    LOOP through each port from start_port to end_port
        IF protocol_type is TCP THEN
            CREATE TCP socket
            SET timeout
            ATTEMPT to connect to port
            IF connection successful THEN
                PRINT "Port is OPEN"
            ELSE
                PRINT "Port is CLOSED or FILTERED"
            END IF
            CLOSE socket

        ELSE IF protocol_type is UDP THEN
            CREATE UDP socket
            SEND test packet
            WAIT for response or timeout
            IF response indicates closed THEN
                PRINT "Port is CLOSED"
            ELSE
                PRINT "Port is OPEN or FILTERED"
            END IF
            CLOSE socket
        END IF
    END LOOP

    PRINT summary of results
END
```

---

## **Key Features to Make it Robust:**

1. **Concurrency**: Use multithreading for high performance when scanning large ranges of ports.
2. **Timeout Handling**: Allow customizable timeouts to handle slow or unreachable hosts.
3. **Logging and Reporting**: Save results for later analysis in multiple formats.
4. **Protocol Support**: Include both **TCP** and **UDP** scanning modes.
5. **Service Detection**: Provide service banners for open ports.
6. **Error Resilience**: Implement error handling for invalid inputs, timeouts, and network failures.
7. **Security Considerations**: Avoid triggering IDS/IPS systems by limiting scanning speed or adding delays.

---
