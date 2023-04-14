# cpu_mem_logger

Log proces taking more than a threshold of cpu or memory, every one sec.
 

## Install

`sudo apt-get install -y libprocps-dev`

## Build

`g++ monitor.cpp -o monitor -lprocps`

## Run and log 

- when cpu usage >50
- memory use >50
- every 1 second

`./monitor usage_log.txt 50 50 1 &`

## Add as systemd service

To run the program in the background even after a restart, you can create a systemd service for it. First, compile the program and move the binary to an appropriate location (e.g., /usr/local/bin).

```bash
g++ monitor3.cpp -o monitor -lprocps
sudo mv monitor /usr/local/bin/monitor
```

Next, create a systemd service file for the program:

```bash
sudo touch /etc/systemd/system/monitor.service
sudo nano /etc/systemd/system/monitor.service
```

Paste the following content into the monitor.service file, replacing CPU_THRESHOLD and MEM_THRESHOLD with the desired values:

```bash
[Unit]
Description=Monitor high CPU and memory usage

[Service]
ExecStart=/usr/local/bin/monitor /var/log/usage_log.txt CPU_THRESHOLD MEM_THRESHOLD
Restart=always
User=nobody
Group=nobody

[Install]
WantedBy=multi-user.target
```

Save and exit the file. Reload the systemd configuration:

```bash
sudo systemctl daemon-reload
```
Enable and start the service:

```bash
sudo systemctl enable monitor
sudo systemctl start monitor
```

Now the program will run in the background and restart automatically upon system restart. To check the status of the service, use the following command:

```bash
sudo systemctl status monitor
```

To stop the service, use:

```bash
sudo systemctl stop monitor
```

And to disable it from starting on boot, use:

```bash
sudo systemctl disable monitor
```


