#include <string.h>

#include "cmsis_os.h"

#include "device_drivers/cli.h"

void cli_device_init(cli_t *dev, UART_HandleTypeDef *huart)
{
    dev->huart = huart;
    dev->index = 0;
    dev->msg_pending = false;
    dev->msg_count = 0;
    dev->msg_proc = 0;
    dev->msg_valid = 0;
    dev->ret = 0;
}

int cli_printline(cli_t *dev, char *line)
{
	static char nl[] = "\r\n";
	HAL_StatusTypeDef ret = 0;

	if(xPortIsInsideInterrupt())
	{
		ret |= HAL_UART_Transmit_IT(dev->huart, (uint8_t *)line, strlen(line));
		ret |= HAL_UART_Transmit_IT(dev->huart, (uint8_t*)nl, strlen(nl));
	}
		else
	{
		ret |= HAL_UART_Transmit(dev->huart, (uint8_t *)line, strlen(line), HAL_MAX_DELAY);
		ret |= HAL_UART_Transmit(dev->huart, (uint8_t *)nl, strlen(nl), HAL_MAX_DELAY);
	}
	return ret;
}

int tokenize(char *s, char *toks[], int maxtoks, char *delim)
{
	int i = 0;

	toks[i] = (char *)strtok(s, delim);
	while(toks[i++] != NULL)
	{
		if(i >= maxtoks - 1) toks[i] = NULL;
		else toks[i] = (char *)strtok(NULL, delim);
	}
	return i - 1;
}
