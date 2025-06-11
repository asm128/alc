APP_3CC         : str           = "ALC";

import os, sys, inspect;
def get_this_filename() -> str : return inspect.getfile(lambda: None).replace('\\', "/");
sys.path.insert(0, "/".join(os.path.dirname(get_this_filename()).split('/')[:-1]));  # Add the parent directory to sys.path

from phyton import initBuildScript;

Import("env");
print("Current CLI targets", COMMAND_LINE_TARGETS);
print("Current Build targets", BUILD_TARGETS);

initBuildScript(env, APP_3CC);
