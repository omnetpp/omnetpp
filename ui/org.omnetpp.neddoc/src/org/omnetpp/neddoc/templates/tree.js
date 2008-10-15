function findChildNode(node, name) 
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
      return node;
    }
    temp = findChildNode(node, name);
    if (temp != null) 
    {
      return temp;
    }
    node = node.nextSibling;
  }
  return null;
}

function toggleFolder(id, imageNode) 
{
  var folder = document.getElementById(id);
  var l = 0;
  var vl = "ftv2vertline.png";
  if (imageNode != null && imageNode.nodeName != "IMG") 
  {
    imageNode = findChildNode(imageNode, "IMG");
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
