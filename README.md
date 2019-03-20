# PyRun

## Goal

Tool to run python with a taylored environnment

## Dependency
* All included in repository.
* Linux compatibility not done yet
* Require C++17 or newer
* Require cmake 3.11 or newer

## Setup
Upon the first run it will try to generate a `pyrun.json` configuration file to run the python file 
Should this step failed, you will have to create pyrun.json with this example:

`
{
	"env": [ # will populate the environment variable with these values
		{
			"base_path": ".\\build\\msvc15-x86\\Release", # non absolute base_path are concatenated with the path to config  
			"var": "PATH", 								  # in this way  "config_directory"/"base_path"/"directory" 
			"directory":
			[ 	
				"", 
				".\\qt4\\bin",
				".\\qt4\\lib\\",
				".\\python27\\bin",
				".\\python27\\DLLs"
			]
		},
		{
			"base_path": ".\\build\\msvc15-x86\\Release",
			"var": "PYTHONPATH",
			"directory":
			[
				"",
				".\\python27\\Lib\\",
				".\\python27\\DLLs\\",
				".\\python27\\Lib\\site-packages\\",
				".\\python27\\Lib\\site-packages\\pywin32_system32\\",
				".\\python27\\Lib\\site-packages\\numpy\\core\\lib",
				".\\python27\\Lib\\site-packages\\PySide",
				".\\python27\\Lib\\site-packages\\PySide\\PySide\\",
				".\\python27\\Lib\\site-packages\\numpy\\linalg\\",
				".\\python27\\Lib\\site-packages\\numpy\\core\\"
			]
		},
		{
			"base_path": "",
			"var": "PYTHONPATH",
			"directory":
			[
				".\\products\\PyModules",
				".\\products\\PyTools"
			]
		}
	],
	"python": ".\\build\\msvc15-x86\\Release\\python27\\bin\\python.exe", # path to a valid python executable
	"includes": [ # allow pyrun to also search in these directory to find py file to execute 
		"" 
	]
}
` 

## Usage
`pyrun <python script>`

* Will try to load a valid `pyrun.json` using `<python script>` directory as a starting point and then looking at parents directories
* Will populate environment variable by concatenating current value with the ones described in `pyrun.json`
* Should `<python script>` not existing in current directory, will try to find a valid candidate as `includes/<python script>`
* Will execute the first valid script found.
