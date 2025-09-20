# DNS Resolver Tool

A high-performance, multi-threaded DNS resolution and HTTP status checking tool written in C++. This tool resolves domain names to IP addresses and performs HTTP/HTTPS requests to check the status of multiple URLs concurrently.

## Features

- **Multi-threaded Processing**: Uses worker threads for concurrent URL processing
- **DNS Resolution**: Resolves domain names to IPv4 addresses
- **HTTP/HTTPS Support**: Handles both HTTP and HTTPS protocols with SSL support
- **Color-coded Output**: Easy-to-read results with color-coded status indicators
- **Timeout Management**: Configurable connection and read timeouts
- **Redirect Handling**: Automatically follows HTTP redirects
- **Thread-safe**: Uses mutexes for safe concurrent operations

## Prerequisites

- C++20 compatible compiler (g++)
- OpenSSL development libraries
- httplib.h (included in the `includes/` directory)

### macOS Installation
```bash
# Install OpenSSL via Homebrew
brew install openssl
```

### Linux Installation
```bash
# Ubuntu/Debian
sudo apt-get install libssl-dev

# CentOS/RHEL
sudo yum install openssl-devel
```

## Building

```bash
# Clone or navigate to the project directory
cd dnsResolv

# Build the project
make

# Clean build files (optional)
make clean
```

## Usage

```bash
./dnsResolv <input_file>
```

### Example
```bash
./dnsResolv links.txt
```

## Input Format

Create a text file with one URL per line. The tool supports both HTTP and HTTPS URLs:

```
http://google.com
https://github.com
http://stackoverflow.com
https://twitter.com
```

## Output Format

The tool provides color-coded output with the following format:
```
[STATUS] URL [IP_ADDRESS] -> HTTP_STATUS_CODE
```

### Status Indicators
- `[+]` (Green) - Successful response (2xx status codes)
- `[*]` (Yellow) - Redirect response (3xx status codes, excluding 304)
- `[-]` (Red) - Error response (4xx, 5xx status codes) or connection failure

### Example Output
```
[+] http://google.com [142.250.191.14] -> 200
[*] http://github.com [140.82.112.4] -> 301 (redirected)
[-] http://nonexistent.example [Unable to resolve] -> Request failed!
```

## Configuration

### Thread Pool Size
The tool automatically determines the optimal number of worker threads (maximum of 8 or the number of URLs, whichever is smaller).

### Timeouts
- Connection timeout: 5 seconds
- Read timeout: 10 seconds

These can be modified in the `worker()` function in `modules/resolveWeb.cpp`.

## Project Structure

```
dnsResolv/
├── main.cpp                 # Entry point
├── modules/
│   ├── resolveWeb.cpp      # Core resolution logic
│   └── resolveWeb.h        # Header file
├── includes/
│   └── httplib.h           # HTTP client library
├── Makefile                # Build configuration
├── links.txt               # Sample input file
└── README.md               # This file
```

## Technical Details

### DNS Resolution
- Uses `getaddrinfo()` system call for DNS resolution
- Supports IPv4 addresses (AF_INET)
- Handles URL preprocessing (strips http:// and https:// prefixes)

### HTTP Client
- Uses httplib for HTTP/HTTPS requests
- SSL/TLS support via OpenSSL
- Automatic redirect following
- Configurable timeouts

### Concurrency
- Thread pool pattern with job queue
- Mutex-protected shared resources
- Thread-safe output printing

## Error Handling

The tool handles various error conditions:
- File not found
- DNS resolution failures
- Network connection timeouts
- SSL/TLS errors
- HTTP request failures

## Performance

- Concurrent processing of multiple URLs
- Efficient memory usage with job queue
- Optimized for bulk URL checking
- Scalable thread pool design

## Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test thoroughly
5. Submit a pull request

## License

This project is open source. Please check the license file for specific terms.

## Troubleshooting

### Common Issues

1. **Compilation Errors**
   - Ensure OpenSSL is properly installed
   - Check that C++20 is supported by your compiler

2. **SSL Certificate Errors**
   - The tool may fail on sites with invalid SSL certificates
   - This is expected behavior for security

3. **Network Timeouts**
   - Increase timeout values in the code if needed
   - Check network connectivity

4. **Permission Errors**
   - Ensure the executable has proper permissions
   - Check file system permissions for input files

## Future Enhancements

- IPv6 support
- Configurable timeout values via command line
- Output format options (JSON, CSV)
- Proxy support
- Custom headers support
- Batch processing optimization
