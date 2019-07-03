# SSDE 

A **S**imple **S**tate **D**iagram **E**ditor.

![](./etc/snapshot.png)

## DOCUMENTATION

Everything should be rather intuitive..

### Editing 

* To **add a state**, select the ![](./src/images/state.png) button in the toolbar and click on the
  canvas

* To **add a transition**, select the ![](./src/images/transition.png) button, click on
  the start state and, keeping the mouse button pressed, go the end state and release mouse button.

* To **add a self transition** (from a state to itself) , select the ![](./src/images/loop.png) button
  and click on start state (the location of the click will decide on that of the
  transition).

* To **add an initial transition**, select the ![](./src/images/initstate.png) button
  and click on initial state 

* To **delete a state or a transition**, select the ![](./src/images/delete.png) button
  and click on the state or transition (deleting a state will also delete all incoming and
  outcoming transitions)

* To **move a state**, select the ![](./src/images/select.png) button and drag the state.

* To **edit a state or a transition**, select the ![](./src/images/select.png) button, click on
  the corresponding item and update the property panel on the right.

### Saving and loading

* The current diagram can be saved by invoking the `Save` or `Save As` action in the `File` menu.

* A saved diagram can be reloaded by invoking the `Open` action in the `File` menu.

* The `New` action in the `File` menu clears the diagram

### Exporting

* The current diagram can be exported to [DOT](http://www.graphviz.org) format by invoking the `Export DOT`
  action in the `Export` menu.

## INSTALLATION

Prebuilt Windows and MacOS versions can be downloaded [here](https://github.com/jserot/ssde/releases/tag/v0.1c)

See the `INSTALL` file to build from sources.

### Credits

This project was inspired by some code written by A. Deterne and L. Malka.
