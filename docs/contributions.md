# About Contributions

## Development Timeline

This checklist is in loose order of what will be finished first to what will be finished last.
There will also likely be a Kanban board on the github.

 - [X] Set up project structure and dependencies.
 - [X] Set up documentation.
 - [ ] Render Backends
	- [ ] Create high level rendering backend mostly abstract base class.
		 > Ensures both backends will have the same high level api, making them more modular and swapable.
		- [ ] Create Progressive Renderer derived from base class.
			> This will 99% likely be a vulkan renderer.
			- [ ] Implement base class methods
			- [ ] Implement Progressive Renderer specific features and methods
				> This will be a low level api that is usable by the end user and in specific parts of the engine for optimizations.
	- [ ] Create Node and Scene classes.
		 > Dont worry about transforms and other components for the classes yet. We will create a component class later once we have implemented global services like model loading and things.
		- [ ] Node Class
			 > The actual things in 3D space like meshes, coliders, lights, particles, etc.
			- [ ] Include internal engine functions for reading and writing certain node data into the node's scene's file.
		- [ ] Scene Class
			> Holds a tree of Nodes, can be saved and loaded from disc.
			- [ ] Make Scene File format for writing scenes to disc to package with the runtime.
	- [ ] Global Services
		 > These singletons handle decoupled and sometimes atomic and/or asyncronous engine processes usually involving IO like 3D model/scene file loading, sound management, Image/Sprite/Texture loading, input devices, etc.
		- [ ] Create 3DAssetLoaderService class singleton.
			 > The service's API should return the model/scene from the 3D file built out into a Scene class instance with a tree of class instances derived from the Node type mirroring the 3D file.  Animations and textures included.
			- [ ] Create high level and model-type-generic ASSIMP interface
		- [ ] Create 2DAssetLoaderService class singleton.
			 > Loads image files for sprites, textures etc.  This service should only load a generic *Image* class which can be used to set textures, sprites, etc.
		- [ ] Create SoundLoaderService class singleton.
			 > Loads sound files.
		- [ ] Create InputDeviceService class singleton.
			 > Includes API for detecting key presses in multiple ways.
	- [ ] Component base class
		 > This is the base class for Node component classes like Transform3D, Transform2D, RigidBody2D, RigidBody3D, etc.
 Virtually all of these component classes will be designed purely for composing the different derived Node types. The end user *MIGHT* be able to attach these
 to Nodes via an `attach_component(Component)` hook or the inspector.
	- [ ] Engine Runtime
		 > This is packaged with the user's end game. Technically all the things before this have been for the runtime and the editor.
	- [ ] Engine editor
		 > Software to make developing a game for the runtime easy.


## More Info

For useful information describing guidelines and engine architecture for contributions see `docs/contributions`