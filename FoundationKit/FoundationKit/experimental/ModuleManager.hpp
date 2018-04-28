/****************************************************************************
  Copyright (c) 2017 libo All rights reserved.
 
  losemymind.libo@gmail.com

****************************************************************************/
#ifndef PAL_MODULEMANAGER_HPP
#define PAL_MODULEMANAGER_HPP



/**
* Utility class for registering modules that are statically linked.
*/
template< class ModuleClass >
class FStaticallyLinkedModuleRegistrant
{
public:

	/**
	* Explicit constructor that registers a statically linked module
	*/
	FStaticallyLinkedModuleRegistrant(const char* InModuleName)
	{
        strvalue = InModuleName;
		//// Create a delegate to our InitializeModule method
		//FModuleManager::FInitializeStaticallyLinkedModule InitializerDelegate = FModuleManager::FInitializeStaticallyLinkedModule::CreateRaw(
		//	this, &FStaticallyLinkedModuleRegistrant<ModuleClass>::InitializeModule);

		//// Register this module
		//FModuleManager::Get().RegisterStaticallyLinkedModule(
		//	FName(InModuleName),	// Module name
		//	InitializerDelegate);	// Initializer delegate
	}

	/**
	* Creates and initializes this statically linked module.
	*
	* The module manager calls this function through the delegate that was created
	* in the @see FStaticallyLinkedModuleRegistrant constructor.
	*
	* @return A pointer to a new instance of the module.
	*/
	IModuleInterface* InitializeModule()
	{
		return new ModuleClass();
	}

    std::string strvalue;
};


#define IS_MONOLITHIC 1

// Boilerplate that is included once for each module, even in monolithic builds
#if !defined(PER_MODULE_BOILERPLATE_ANYLINK)
#define PER_MODULE_BOILERPLATE_ANYLINK(ModuleImplClass, ModuleName)
#endif

/**
* Module implementation boilerplate for regular modules.
*
* This macro is used to expose a module's main class to the rest of the engine.
* You must use this macro in one of your modules C++ modules, in order for the 'InitializeModule'
* function to be declared in such a way that the engine can find it. Also, this macro will handle
* the case where a module is statically linked with the engine instead of dynamically loaded.
*
* This macro is intended for modules that do NOT contain gameplay code.
* If your module does contain game classes, use IMPLEMENT_GAME_MODULE instead.
*
* Usage:   IMPLEMENT_MODULE(<My Module Class>, <Module name string>)
*
* @see IMPLEMENT_GAME_MODULE
*/
#if IS_MONOLITHIC

// If we're linking monolithically we assume all modules are linked in with the main binary.
#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
		/** Global registrant object for this module when linked statically */ \
		static FStaticallyLinkedModuleRegistrant< ModuleImplClass > ModuleRegistrant##ModuleName( #ModuleName ); \
		/** Implement an empty function so that if this module is built as a statically linked lib, */ \
		/** static initialization for this lib can be forced by referencing this symbol */ \
		void EmptyLinkFunctionForStaticInitialization##ModuleName(){} \
		PER_MODULE_BOILERPLATE_ANYLINK(ModuleImplClass, ModuleName)

#else

#define IMPLEMENT_MODULE( ModuleImplClass, ModuleName ) \
		\
		/**/ \
		/* InitializeModule function, called by module manager after this module's DLL has been loaded */ \
		/**/ \
		/* @return	Returns an instance of this module */ \
		/**/ \
		extern "C" DLLEXPORT IModuleInterface* InitializeModule() \
		{ \
			return new ModuleImplClass(); \
		} \
		PER_MODULE_BOILERPLATE \
		PER_MODULE_BOILERPLATE_ANYLINK(ModuleImplClass, ModuleName)

#endif //IS_MONOLITHIC

#endif // END OF PAL_MODULEMANAGER_HPP