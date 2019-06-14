# -*- coding: utf-8 -*-
"""
/***************************************************************************
 globalfate
                                 A QGIS plugin
 Simulates contaminants fate and transport through the global river network
                             -------------------
        begin                : 2018-09-25
        copyright            : (C) 2018 by Carme Font
        email                : cfont@icra.cat
        git sha              : $Format:%H$
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
 This script initializes the plugin, making it known to QGIS.
"""


# noinspection PyPep8Naming
def classFactory(iface):  # pylint: disable=invalid-name
    """Load globalfate class from file globalfate.

    :param iface: A QGIS interface instance.
    :type iface: QgisInterface
    """
    #
    from .globalfate import globalfate
    return globalfate(iface)
