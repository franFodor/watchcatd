# watchcatd

Simple daemon to keep track of changes on a file. Changes being access, 
modify and delete.

## Installation

Run simply with make:

```bash
make all  
```

After running with `./watchcatd <FILE_PATH>` (you can try `.watchcatd 
src/test.txt` for instance), it will start a daemon. To kill it, you will have 
to find the process and kill it. You can use the following command to get PID 
`ps -ef | grep watchcatd` and then kill -9 PID to kill it. 
