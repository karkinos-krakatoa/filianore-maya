import os
import logging


def __createLogger():
    logger = logging.getLogger(__name__)
    logLevel = os.environ.get('FILIANORE_MAYA_LOG_LEVEL', 'warning')

    if logLevel == 'debug':
        logger.setLevel(logging.DEBUG)
    elif logLevel == 'info':
        logger.setLevel(logging.INFO)
    elif logLevel == 'warning':
        logger.setLevel(logging.WARNING)
    elif logLevel == 'error':
        logger.setLevel(logging.ERROR)
    else:
        logger.setLevel(logging.INFO)

    return logger

logger = __createLogger()