# cpu_mem_logger

Log proces taking more than a threshold of cpu or memory, every one sec.
 

## Install

`sudo apt-get install -y libprocps-dev`

## build

`g++ monitor.cpp -o monitor -lprocps`

## run and log 

- when cpu usage >50
- memory use >50
- every 1 second

`./monitor usage_log.txt 50 50 1 &`





