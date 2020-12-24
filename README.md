# DA
Dtk (Deepin Toolkit) addons library

##What is it?
That's a library to have many components easily extensible, fast to handle and documented in the headers for your favorite DE and your comprehension when you search a class member. The classes in this library are not officially part of Dtk, but is a suite of addons. It can handle mouse and finger gestures faster and easily, with a template class integrated (SwipingGesture with VisualStackElement) or extend exisiting components part of Dtk (like DBlurEffect -> EffectWidget that has more in-window effects, or SwipingGestures that enhances Qt's QGesture's family). This is (for the moment) built as a trick library to have somethings already made (like DSplittedWindow nor it bar), or the auto debugging DApplication derivate (DAddonApplication).

##What it should not
- Lead only by my needs, as it has been, even if it could be bad.
- Be maintained, enhanced, updated only by me
- Only for my own interest

##How to build
Go in the source directory and then:
```
$ qmake
$ make
```
If you want to install it:
```
# make install
```
If you haven't the capacity to build it, a compiled version should be released yet.
