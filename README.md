To set up your local working directory for FloorOS, we recommend Linux for it's ease of development and compilation!  
We also recommend `make` as the build system! If you go with it, there's a preset [Makefile](./Makefile) which orchestrates the whole process.


You can choose from any of the following, as they all produce FloorOS in some form:

**Option A**  
To build and run a bootable ISO:
```bash
make run-iso
```

**Option B**  
To directly run FloorOS:
```bash
make run
```

**Option C**  
To run FloorOS directly inside your host terminal:
```bash
make run-dev
```

**Option D**  
And lastly, to build FloorOS to a bootable ISO:
```bash
make FloorOS.iso
```
