import requests

# The server URL. We use "localhost" because this script is running
# on the same machine as the server.
# The port is 8080, as we defined when starting the server.
server_url = "http://localhost:8080/"

# The same hardcoded scramble string from our Android app example
cube_string = "DUUBULDBFRBFRRULLLBRDFFFBLURDBFDFDRFRULBLUFDURRBLBDUDL"

# The full URL for the request
full_url = server_url + cube_string

print(f"[*] Sending request to: {full_url}")

try:
    # Make the GET request to the server
    response = requests.get(full_url, timeout=5) # 5-second timeout

    # Check if the request was successful (HTTP status code 200)
    if response.status_code == 200:
        solution = response.text
        print("\n[+] Success! Server responded.")
        print(f"    Solution: {solution}")
    else:
        print(f"\n[-] Error: Server returned status code {response.status_code}")

except requests.exceptions.RequestException as e:
    # This block runs if the connection fails (e.g., server not running)
    print("\n[-] Connection Failed.")
    print(f"    Error details: {e}")
    print("\n    Please make sure your 'start_server.py' is running in another terminal.")