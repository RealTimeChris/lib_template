import os
import sys

def rename_template_parts(root_dir, new_name):
    new_name_lower = new_name.lower()
    new_name_upper = new_name.upper()

    for root, dirs, files in os.walk(root_dir, topdown=False):

        for name in files:
            old_path = os.path.join(root, name)
            
            try:
                with open(old_path, 'r', encoding='utf-8', errors='ignore') as f:
                    content = f.read()

                updated_content = content.replace('lt_', f"{new_name_lower}_")
                updated_content = updated_content.replace('lt-', f"{new_name_lower}-")
                updated_content = updated_content.replace('LT_', f"{new_name_upper}_")
                updated_content = updated_content.replace('lib_template', new_name_lower)
                
                if updated_content != content:
                    with open(old_path, 'w', encoding='utf-8') as f:
                        f.write(updated_content)
            except Exception as e:
                print(f"Could not update content of {name}: {e}")

            new_file_name = name.replace('lt_', f"{new_name_lower}_")
            new_file_name = new_file_name.replace('lt-', f"{new_name_lower}-")
            new_file_name = new_file_name.replace('LT_', f"{new_name_upper}_")
            new_file_name = new_file_name.replace('lib_template', new_name_lower)
            
            if new_file_name != name:
                new_path = os.path.join(root, new_file_name)
                os.rename(old_path, new_path)
                print(f"Renamed file: {name} -> {new_file_name}")

        for name in dirs:
            old_path = os.path.join(root, name)

            new_dir_name = name.replace('lt_', f"{new_name_lower}_")
            new_dir_name = new_dir_name.replace('lt-', f"{new_name_lower}-")
            new_dir_name = new_dir_name.replace('LT_', f"{new_name_upper}_")
            new_dir_name = new_dir_name.replace('lib_template', new_name_lower)
            
            if new_dir_name != name:
                new_path = os.path.join(root, new_dir_name)
                os.rename(old_path, new_path)
                print(f"Renamed directory: {name} -> {new_dir_name}")

if __name__ == "__main__":
    if len(sys.argv) < 2:
        print("Usage: python rename_script.py <NewPrefix>")
        sys.exit(1)
    
    target_prefix = sys.argv[1]
    rename_template_parts('.', target_prefix)
    print("\nRefactor complete.")