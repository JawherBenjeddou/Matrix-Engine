import subprocess
import os
import shutil

def install_dependencies():
    # Define the path to the Dependencies folder
    dependencies_folder = "Dependencies"

    # Define a list of GitHub repositories and their respective branches or versions
    #(URL, branch_option, branch_name)
    github_repositories = [
        ("https://github.com/nothings/stb",  "--branch", "master"),  # No specific branch/version for this repository
        ("https://github.com/g-truc/glm", "--branch", "0.9.9.8"),
        ("https://github.com/ocornut/imgui", "--branch", "v1.89.9-docking"),
        ("https://github.com/gabime/spdlog", "--branch", "v1.12.0")
    ]

    try:
        # Create the Dependencies folder if it doesn't exist
        #if not os.path.exists(dependencies_folder):
            #os.makedirs(dependencies_folder)
        #if the dependencies exists remove them
          # Remove existing dependencies if they exist
        for repo_url, branch_option, branch_name in github_repositories:
            repo_name = repo_url.split('/')[-1]
            repo_path = os.path.join(dependencies_folder, repo_name)
            if os.path.exists(repo_path):
                os.rmdir(repo_path)  # Remove the directory if it's empty
            # Print a message indicating the removal
            print(f"Directory {repo_name} has been removed.")

            # Construct the git clone command based on branch information
            git_command = ["git", "clone", repo_url]
            if branch_option and branch_name:
                git_command.extend([branch_option, branch_name])

            process = subprocess.Popen(
                git_command,
                cwd=dependencies_folder,
                stdout=subprocess.PIPE,
                stderr=subprocess.STDOUT,
                text=True  # Ensure text mode for output
            )

            # Print the real-time output
            for line in process.stdout:
                print(line, end='')

            process.wait()  # Wait for the process to complete

            if process.returncode == 0:
                print(f"Successfully cloned {repo_url}")
            else:
                print(f"Failed to clone {repo_url}")
    except Exception as e:
        print(f"Error: {e}")
        print("Failed to install dependencies.")

if __name__ == "__main__":
    install_dependencies()
