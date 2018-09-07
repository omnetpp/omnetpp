function findDescendantNode(node, name, index)
{
  var temp;
  if (node == null)
  {
    return null;
  }
  node = node.firstChild;
  while (node != null)
  {
    if (node.nodeName == name)
    {
      if (index == 0)
        return node;
      else
        index--;
    }
    temp = findDescendantNode(node, name, index);
    if (temp != null)
    {
      return temp;
    }
    node = node.nextSibling;
  }
  return null;
}

function setupTree(id, lastNodes)
{
  var last = true;
  var node = document.getElementById(id);
  if (node != null)
  {
    node = node.lastChild;
    while (node != null)
    {
      if (node.nodeName == "P")
      {
        imageNode = findDescendantNode(node, "IMG", lastNodes.length);
        if (imageNode != null)
        {
          // find out whether this is a container or a simple node
          var p = "p";
          var contentNode = imageNode;
          while (contentNode != null)
          {
            if (contentNode.nodeName == "A")
            {
              p = "";
              break;
            }
            contentNode = contentNode.nextSibling;
          }
          // set the image
          if (last)
          {
            imageNode.src = "ftv2"+p+"lastnode.png";
            last = false;
          }
          else
          {
            imageNode.src = "ftv2"+p+"node.png";
          }
          var i = lastNodes.length;
          imageNode = imageNode.previousSibling;
          while (imageNode != null)
          {
            if (imageNode.nodeName == "IMG")
            {
              if (lastNodes[--i])
              {
                imageNode.src = "ftv2blank.png";
              }
            }
            imageNode = imageNode.previousSibling;
          }
        }
      }
      else if (node.nodeName == "DIV")
      {
        var newLastNodes = lastNodes.slice(0);
        newLastNodes.push(last);
        setupTree(node.id, newLastNodes);
      }
      node = node.previousSibling;
    }
  }
}

function toggleFolder(id, imageNode)
{
  var folder = document.getElementById(id);
  var l = 0;
  var vl = "ftv2vertline.png";
  if (imageNode != null && imageNode.nodeName != "IMG")
  {
    imageNode = findDescendantNode(imageNode, "IMG", 0);
    if (imageNode!=null) l = imageNode.src.length;
  }
  if (folder == null)
  {
  }
  else if (folder.style.display == "block")
  {
    while (imageNode != null &&
           imageNode.src.substring(l-vl.length,l) == vl)
    {
      imageNode = imageNode.nextSibling;
      l = imageNode.src.length;
    }
    if (imageNode != null)
    {
      l = imageNode.src.length;
      imageNode.nextSibling.src = "ftv2folderclosed.png";
      if (imageNode.src.substring(l-13,l) == "ftv2mnode.png")
      {
        imageNode.src = "ftv2pnode.png";
      }
      else if (imageNode.src.substring(l-17,l) == "ftv2mlastnode.png")
      {
        imageNode.src = "ftv2plastnode.png";
      }
    }
    folder.style.display = "none";
  }
  else
  {
    while (imageNode != null &&
           imageNode.src.substring(l-vl.length,l) == vl)
    {
      imageNode = imageNode.nextSibling;
      l = imageNode.src.length;
    }
    if (imageNode != null)
    {
      l = imageNode.src.length;
      imageNode.nextSibling.src = "ftv2folderopen.png";
      if (imageNode.src.substring(l-13,l) == "ftv2pnode.png")
      {
        imageNode.src = "ftv2mnode.png";
      }
      else if (imageNode.src.substring(l-17,l) == "ftv2plastnode.png")
      {
        imageNode.src = "ftv2mlastnode.png";
      }
    }
    folder.style.display = "block";
  }
}
