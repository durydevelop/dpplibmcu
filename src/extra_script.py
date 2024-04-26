Import("env")


def skip_from_build(node):
    print("Get: "+node.get_path())
    if "src\\dmpacket" in node.get_path() or "src\\dvector" in node.get_path():
        print("Add: "+node.get_path())
        return node

    return None

# Register callback
env.AddBuildMiddleware(skip_from_build, "*")