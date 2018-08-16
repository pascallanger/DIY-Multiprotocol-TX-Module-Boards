# Building the Arduino IDE Multi 4-in-1 boards

**Note:** It is essential to build the compressed archives in a Linux environment in order to preserve the file modes for Linux scripts and executables.

1. Check out or clone this project from https://github.com/pascallanger/DIY-Multiprotocol-TX-Module-Boards
1. Create and check out a new branch for your changes
1. Make the necessary changes to the files in the platform sub-folders, `source/avr` or `source/stm32`
1. Ensure that the version number is incremented in the `platform.txt` file for each updated platform (don't increment it if no changes were made to that platform)
1. Run the `source/make_archives.sh` script to create the compressed archives for each platform
1. Add a new section in `package_multi_4in1_board_index.json` for the version
    1. Use the output from `make_archives.sh` to populate the `checksum` and `size` properties
1. Inspect the changes in your Git client.  
    1. **If only one platform was modified, discard the new archive for the platform which is _not_ being updated**. This avoids the need to unnecessarily update the SHA hash for the archive which has not actually been changed.
1. Push your new branch to the remote
1. Create a Pull Request against the master branch on https://github.com/pascallanger/DIY-Multiprotocol-TX-Module-Boards
