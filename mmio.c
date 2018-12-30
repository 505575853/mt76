/*
 * Copyright (C) 2016 Felix Fietkau <nbd@nbd.name>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "mt76.h"
#include "trace.h"

u32 mt76_mmio_rr(struct mt76_dev *dev, u32 offset)
{
	u32 val;

	val = ioread32(dev->mmio.regs + offset);
	trace_reg_rr(dev, offset, val);

	return val;
}
EXPORT_SYMBOL_GPL(mt76_mmio_rr);

void mt76_mmio_wr(struct mt76_dev *dev, u32 offset, u32 val)
{
	trace_reg_wr(dev, offset, val);
	iowrite32(val, dev->mmio.regs + offset);
}
EXPORT_SYMBOL_GPL(mt76_mmio_wr);

u32 mt76_mmio_rmw(struct mt76_dev *dev, u32 offset, u32 mask, u32 val)
{
	val |= mt76_mmio_rr(dev, offset) & ~mask;
	mt76_mmio_wr(dev, offset, val);
	return val;
}
EXPORT_SYMBOL_GPL(mt76_mmio_rmw);

void mt76_mmio_copy(struct mt76_dev *dev, u32 offset,
		    const void *data, int len)
{
	__iowrite32_copy(dev->mmio.regs + offset, data, len >> 2);
}
EXPORT_SYMBOL_GPL(mt76_mmio_copy);

int mt76_mmio_wr_rp(struct mt76_dev *dev, u32 base,
		    const struct mt76_reg_pair *data, int len)
{
	while (len > 0) {
		mt76_mmio_wr(dev, data->reg, data->value);
		data++;
		len--;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(mt76_mmio_wr_rp);

int mt76_mmio_rd_rp(struct mt76_dev *dev, u32 base,
		    struct mt76_reg_pair *data, int len)
{
	while (len > 0) {
		data->value = mt76_mmio_rr(dev, data->reg);
		data++;
		len--;
	}

	return 0;
}
EXPORT_SYMBOL_GPL(mt76_mmio_rd_rp);

void mt76_mmio_init(struct mt76_dev *dev, void __iomem *regs,
		    const struct mt76_bus_ops *ops)
{
	dev->bus = ops;
	dev->mmio.regs = regs;

	skb_queue_head_init(&dev->mmio.mcu.res_q);
	init_waitqueue_head(&dev->mmio.mcu.wait);
	spin_lock_init(&dev->mmio.irq_lock);
	mutex_init(&dev->mmio.mcu.mutex);
}
EXPORT_SYMBOL_GPL(mt76_mmio_init);
