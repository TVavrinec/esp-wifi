Import('env')

print(env.subst("$mklittlefs_path"))
fs_size = "8MB"
data_dir = "data"

if "buildfs" in COMMAND_LINE_TARGETS:
    env.Execute(
        env.VerboseAction(
            f"python $mklittlefs_path -c -p {fs_size} -s 0x10000 .pio/build/$PIOENV/{data_dir} .pio/build/$PIOENV/spiffs.bin",
            "Building FS image"
        )
    )

# do not run this script as a normal SConscript
Exit(0)