#!/bin/bash

# Define the path to your local Git repository
REPO_PATH="."

# Change to the repository directory
cd "$REPO_PATH" || { echo "Failed to cd into $REPO_PATH"; exit 1; }

# Fetch the latest changes from the remote repository
git fetch origin

# Check if there are any updates on the remote branch
LOCAL_HASH=$(git rev-parse @)
REMOTE_HASH=$(git rev-parse @{u})

if [ "$LOCAL_HASH" != "$REMOTE_HASH" ]; then
    echo "Updates found. Pulling changes..."
    git pull origin main || { echo "Failed to pull changes"; exit 1; }

    echo "Building the project..."
    make || { echo "Make failed"; exit 1; }

    echo "Installing the project..."
    sudo make install || { echo "Make install failed"; exit 1; }

    echo "Update and installation complete."
else
    echo "No updates found."
fi
