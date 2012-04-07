// navtree.js: 

var NAVTREE =
[
###################################################
  [ "Lua_icxx Usage", "index.html", [
    [ "Installation", "install.html", null ],
    [ "Lua Sandboxes", "sandboxing.html", null ],
    [ "TODO List", "todo.html", null ]
  ] ],
  [ "Lua_icxx Ref", "index.html", [
    [ "Class List", "annotated.html", null ], 
    [ "Class Hierarchy", "inherits.html", null ],
    [ "Class Members", "functions.html", null ],
    [ "File List", "files.html", null ]
  ] ]
###################################################
];

function getNode(o, po)
{
  po.childrenVisited = true;
  var l = po.childrenData.length-1;
  for (var i in po.childrenData) 
  {
    var nodeData = po.childrenData[i];
    po.children[i] = newNode(o, po, nodeData[0], nodeData[1], nodeData[2],
        i==l);
###################################################
    expandNode(o, po.children[i], true)
###################################################
  }
}

// inherits.html: 
<script type="text/javascript">
###################################################
  initNavTree('inherits.html','');
###################################################
</script>


// navtree.css: 
#side-nav {
###################################################
  width: 200px;
###################################################
}
