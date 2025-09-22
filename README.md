# Download
```
git clone --recurse-submodules link
```

## Install Depedencies
Run script:
```
.\build_dependencies.sh
```

# Install voxblox

```
.\bd.sh
```

# Use voxblox

```
find_package(voxblox)

target_link_libraris( your_lib  ${voxblox_libraries})
```