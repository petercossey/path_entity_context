Path Entity Context
===================

Module provides a CTools context that loads an entity of given type from a given path argument.

E.g. if current path is user/%user/orders/%commerce_order, the module allows to obtain commerce order
object from the fourth path argument.


Installation
------------

Install the module as usual.


Usage
-----

The module adds one new context per entity type (e.g. "Node from path", "User from path"). Context settings form allows
to select the path argument position to use as entity ID.

Arguments numeration starts with 1, not 0.

By default argument position can be between 1 and 4. If it's not enough, one can easily extend the upper limit by setting
"path_entity_context_limit" variable.
