import os
cmd = """
osascript -e 'tell application "System Events" to keystroke "m" using {command down}' 
"""
# minimize active window
os.system(cmd)
