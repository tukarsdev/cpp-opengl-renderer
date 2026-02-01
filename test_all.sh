#!/bin/bash
set -e

# Define all available distros
ALL_DISTROS=(
    "ubuntu:latest"
    "fedora:latest"
    "archlinux:latest"
)

# Filter distros if an argument is provided
SELECTED_DISTRO=$1
DISTROS=()

if [ -n "$SELECTED_DISTRO" ]; then
    for distro in "${ALL_DISTROS[@]}"; do
        if [[ $distro == *"$SELECTED_DISTRO"* ]]; then
            DISTROS+=("$distro")
        fi
    done

    if [ ${#DISTROS[@]} -eq 0 ]; then
        echo "Error: Distro '$SELECTED_DISTRO' not found in list."
        echo "Available: ubuntu, fedora, archlinux"
        exit 1
    fi
else
    DISTROS=("${ALL_DISTROS[@]}")
fi

for image in "${DISTROS[@]}"; do
    name=$(echo $image | cut -d: -f1)

    echo "------------------------------------------------"
    echo "Testing on $name ($image)..."
    echo "------------------------------------------------"

    # Build the specific image
    docker build -t "renderer-test-$name" \
        --build-arg BASE_IMAGE="$image" .

    # Run the container
    # -i is used to support the dependency installation prompt
    docker run --rm -i "renderer-test-$name"

    echo -e "\n✓ $name passed\n"
done

echo "Tests completed successfully!"
