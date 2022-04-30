from robot.api.deco import keyword


ROBOT_AUTO_KEYWORDS = False


@keyword('Create Dict From Lists')
def create_dict_from_lists(keys, values):
    return dict(zip(keys, values))

