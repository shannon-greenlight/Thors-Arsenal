# import pcbnew

# print("Script started running.")
# Your script content goes here

def toggle_lock_on_selected_components():
    print("Toggling lock on selected components...")  # Print statement added for verification
    # Access the current PCB
    board = pcbnew.GetBoard()
    
    # Iterate over all modules (components) in the board
    for module in board.GetModules():
        # Check if the module is selected
        if module.IsSelected():
            # Toggle the lock state
            if module.IsLocked():
                module.SetLocked(False)
                print(f"Unlocked: {module.GetReference()}")  # Print unlocked components
            else:
                module.SetLocked(True)
                print(f"Locked: {module.GetReference()}")  # Print locked components
    
    # Refresh to show changes
    pcbnew.Refresh()

# Call the function