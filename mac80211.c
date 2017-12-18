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
#include <linux/of.h>
#include "mt76.h"

#define CHAN2G(_idx, _freq) {			\
	.band = NL80211_BAND_2GHZ,		\
	.center_freq = (_freq),			\
	.hw_value = (_idx),			\
	.max_power = 30,			\
}

#define CHAN5G(_idx, _freq) {			\
	.band = NL80211_BAND_5GHZ,		\
	.center_freq = (_freq),			\
	.hw_value = (_idx),			\
	.max_power = 30,			\
}

static const struct ieee80211_channel mt76_channels_2ghz[] = {
	CHAN2G(1, 2412),
	CHAN2G(2, 2417),
	CHAN2G(3, 2422),
	CHAN2G(4, 2427),
	CHAN2G(5, 2432),
	CHAN2G(6, 2437),
	CHAN2G(7, 2442),
	CHAN2G(8, 2447),
	CHAN2G(9, 2452),
	CHAN2G(10, 2457),
	CHAN2G(11, 2462),
	CHAN2G(12, 2467),
	CHAN2G(13, 2472),
	CHAN2G(14, 2484),
};

static const struct ieee80211_channel mt76_channels_5ghz[] = {
	CHAN5G(36, 5180),
	CHAN5G(40, 5200),
	CHAN5G(44, 5220),
	CHAN5G(48, 5240),

	CHAN5G(52, 5260),
	CHAN5G(56, 5280),
	CHAN5G(60, 5300),
	CHAN5G(64, 5320),

	CHAN5G(100, 5500),
	CHAN5G(104, 5520),
	CHAN5G(108, 5540),
	CHAN5G(112, 5560),
	CHAN5G(116, 5580),
	CHAN5G(120, 5600),
	CHAN5G(124, 5620),
	CHAN5G(128, 5640),
	CHAN5G(132, 5660),
	CHAN5G(136, 5680),
	CHAN5G(140, 5700),

	CHAN5G(149, 5745),
	CHAN5G(153, 5765),
	CHAN5G(157, 5785),
	CHAN5G(161, 5805),
	CHAN5G(165, 5825),
};

static const struct ieee80211_tpt_blink mt76_tpt_blink[] = {
	{ .throughput =   0 * 1024, .blink_time = 334 },
	{ .throughput =   1 * 1024, .blink_time = 260 },
	{ .throughput =   5 * 1024, .blink_time = 220 },
	{ .throughput =  10 * 1024, .blink_time = 190 },
	{ .throughput =  20 * 1024, .blink_time = 170 },
	{ .throughput =  50 * 1024, .blink_time = 150 },
	{ .throughput =  70 * 1024, .blink_time = 130 },
	{ .throughput = 100 * 1024, .blink_time = 110 },
	{ .throughput = 200 * 1024, .blink_time =  80 },
	{ .throughput = 300 * 1024, .blink_time =  50 },
};

static int mt76_led_init(struct mt76_dev *dev)
{
	struct device_node *np = dev->dev->of_node;
	struct ieee80211_hw *hw = dev->hw;
	int led_pin;

	if (!dev->led_cdev.brightness_set && !dev->led_cdev.blink_set)
		return 0;

	snprintf(dev->led_name, sizeof(dev->led_name),
		 "mt76-%s", wiphy_name(hw->wiphy));

	dev->led_cdev.name = dev->led_name;
	dev->led_cdev.default_trigger =
		ieee80211_create_tpt_led_trigger(hw,
					IEEE80211_TPT_LEDTRIG_FL_RADIO,
					mt76_tpt_blink,
					ARRAY_SIZE(mt76_tpt_blink));

	np = of_get_child_by_name(np, "led");
	if (np) {
		if (!of_property_read_u32(np, "led-sources", &led_pin))
			dev->led_pin = led_pin;
		dev->led_al = of_property_read_bool(np, "led-active-low");
	}

	return devm_led_classdev_register(dev->dev, &dev->led_cdev);
}

void mt76_set_ht_cap(struct mt76_dev *dev,
		     struct ieee80211_sta_ht_cap *ht_cap)
{
	int i, nstream = __sw_hweight8(dev->antenna_mask);

	ht_cap->ht_supported = true;
	ht_cap->cap |= IEEE80211_HT_CAP_SUP_WIDTH_20_40 |
		       IEEE80211_HT_CAP_GRN_FLD |
		       IEEE80211_HT_CAP_SGI_20 |
		       IEEE80211_HT_CAP_SGI_40 |
		       (1 << IEEE80211_HT_CAP_RX_STBC_SHIFT);
	if (nstream > 1)
		ht_cap->cap |= IEEE80211_HT_CAP_TX_STBC;
	else
		ht_cap->cap &= ~IEEE80211_HT_CAP_TX_STBC;

	for (i = 0; i < IEEE80211_HT_MCS_MASK_LEN; i++)
		ht_cap->mcs.rx_mask[i] = (i < nstream) ? 0xff : 0;
	ht_cap->mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED;

	ht_cap->ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K;
	ht_cap->ampdu_density = IEEE80211_HT_MPDU_DENSITY_4;
}
EXPORT_SYMBOL_GPL(mt76_set_ht_cap);

void mt76_set_vht_cap(struct mt76_dev *dev,
		      struct ieee80211_sta_vht_cap *vht_cap)
{
	int i, nstream = __sw_hweight8(dev->antenna_mask);
	u16 mcs_map = 0;

	vht_cap->vht_supported = true;

	for (i = 0; i < 8; i++) {
		if (i < nstream)
			mcs_map |= (IEEE80211_VHT_MCS_SUPPORT_0_9 << (i * 2));
		else
			mcs_map |=
				(IEEE80211_VHT_MCS_NOT_SUPPORTED << (i * 2));
	}

	vht_cap->vht_mcs.rx_mcs_map = cpu_to_le16(mcs_map);
	vht_cap->vht_mcs.tx_mcs_map = cpu_to_le16(mcs_map);
	vht_cap->cap |= IEEE80211_VHT_CAP_RXLDPC |
			IEEE80211_VHT_CAP_RXSTBC_1 |
			IEEE80211_VHT_CAP_SHORT_GI_80;
	if (nstream > 1)
		vht_cap->cap |= IEEE80211_VHT_CAP_TXSTBC;
	else
		vht_cap->cap &= ~IEEE80211_VHT_CAP_TXSTBC;
}
EXPORT_SYMBOL_GPL(mt76_set_vht_cap);

static int
mt76_init_sband(struct mt76_dev *dev, struct mt76_sband *msband,
		const struct ieee80211_channel *chan, int n_chan,
		struct ieee80211_rate *rates, int n_rates, bool vht)
{
	struct ieee80211_supported_band *sband = &msband->sband;
	void *chanlist;
	int size;

	size = n_chan * sizeof(*chan);
	chanlist = devm_kmemdup(dev->dev, chan, size, GFP_KERNEL);
	if (!chanlist)
		return -ENOMEM;

	msband->chan = devm_kzalloc(dev->dev, n_chan * sizeof(*msband->chan),
				    GFP_KERNEL);
	if (!msband->chan)
		return -ENOMEM;

	sband->channels = chanlist;
	sband->n_channels = n_chan;
	sband->bitrates = rates;
	sband->n_bitrates = n_rates;
	dev->chandef.chan = &sband->channels[0];

	mt76_set_ht_cap(dev, &sband->ht_cap);
	if (vht)
		mt76_set_vht_cap(dev, &sband->vht_cap);

	return 0;
}

static int
mt76_init_sband_2g(struct mt76_dev *dev, struct ieee80211_rate *rates,
		   int n_rates)
{
	dev->hw->wiphy->bands[NL80211_BAND_2GHZ] = &dev->sband_2g.sband;

	return mt76_init_sband(dev, &dev->sband_2g,
			       mt76_channels_2ghz,
			       ARRAY_SIZE(mt76_channels_2ghz),
			       rates, n_rates, false);
}

static int
mt76_init_sband_5g(struct mt76_dev *dev, struct ieee80211_rate *rates,
		   int n_rates, bool vht)
{
	dev->hw->wiphy->bands[NL80211_BAND_5GHZ] = &dev->sband_5g.sband;

	return mt76_init_sband(dev, &dev->sband_5g,
			       mt76_channels_5ghz,
			       ARRAY_SIZE(mt76_channels_5ghz),
			       rates, n_rates, vht);
}

static void
mt76_check_sband(struct mt76_dev *dev, int band)
{
	struct ieee80211_supported_band *sband = dev->hw->wiphy->bands[band];
	bool found = false;
	int i;

	if (!sband)
		return;

	for (i = 0; i < sband->n_channels; i++) {
		if (sband->channels[i].flags & IEEE80211_CHAN_DISABLED)
			continue;

		found = true;
		break;
	}

	if (found)
		return;

	sband->n_channels = 0;
	dev->hw->wiphy->bands[band] = NULL;
}

int mt76_register_device(struct mt76_dev *dev, bool vht,
			 struct ieee80211_rate *rates, int n_rates)
{
	struct ieee80211_hw *hw = dev->hw;
	struct wiphy *wiphy = hw->wiphy;
	int ret;

	dev_set_drvdata(dev->dev, dev);

	spin_lock_init(&dev->lock);
	spin_lock_init(&dev->cc_lock);
	INIT_LIST_HEAD(&dev->txwi_cache);

	SET_IEEE80211_DEV(hw, dev->dev);
	SET_IEEE80211_PERM_ADDR(hw, dev->macaddr);

	wiphy->interface_modes =
		BIT(NL80211_IFTYPE_STATION) |
		BIT(NL80211_IFTYPE_AP) |
#ifdef CONFIG_MAC80211_MESH
		BIT(NL80211_IFTYPE_MESH_POINT) |
#endif
		BIT(NL80211_IFTYPE_ADHOC);

	wiphy->features |= NL80211_FEATURE_ACTIVE_MONITOR;

	wiphy->available_antennas_tx = 3;
	wiphy->available_antennas_rx = 3;
	dev->antenna_mask = 3;

	hw->txq_data_size = sizeof(struct mt76_txq);
	hw->max_tx_fragments = 16;

	ieee80211_hw_set(hw, SIGNAL_DBM);
	ieee80211_hw_set(hw, PS_NULLFUNC_STACK);
	ieee80211_hw_set(hw, HOST_BROADCAST_PS_BUFFERING);
	ieee80211_hw_set(hw, AMPDU_AGGREGATION);
	ieee80211_hw_set(hw, SUPPORTS_RC_TABLE);
	ieee80211_hw_set(hw, SUPPORT_FAST_XMIT);
	ieee80211_hw_set(hw, SUPPORTS_CLONED_SKBS);
	ieee80211_hw_set(hw, SUPPORTS_AMSDU_IN_AMPDU);
	ieee80211_hw_set(hw, TX_AMSDU);
	ieee80211_hw_set(hw, TX_FRAG_LIST);
	ieee80211_hw_set(hw, MFP_CAPABLE);

	wiphy->flags |= WIPHY_FLAG_IBSS_RSN;

	if (dev->cap.has_2ghz) {
		ret = mt76_init_sband_2g(dev, rates, n_rates);
		if (ret)
			return ret;
	}

	if (dev->cap.has_5ghz) {
		ret = mt76_init_sband_5g(dev, rates + 4, n_rates - 4, vht);
		if (ret)
			return ret;
	}

	wiphy_read_of_freq_limits(dev->hw->wiphy);
	mt76_check_sband(dev, NL80211_BAND_2GHZ);
	mt76_check_sband(dev, NL80211_BAND_5GHZ);

	ret = mt76_led_init(dev);
	if (ret)
		return ret;

	return ieee80211_register_hw(hw);
}
EXPORT_SYMBOL_GPL(mt76_register_device);

void mt76_unregister_device(struct mt76_dev *dev)
{
	struct ieee80211_hw *hw = dev->hw;

	ieee80211_unregister_hw(hw);
	mt76_tx_free(dev);
}
EXPORT_SYMBOL_GPL(mt76_unregister_device);

void mt76_rx(struct mt76_dev *dev, enum mt76_rxq_id q, struct sk_buff *skb)
{
	if (!test_bit(MT76_STATE_RUNNING, &dev->state)) {
		dev_kfree_skb(skb);
		return;
	}

	__skb_queue_tail(&dev->rx_skb[q], skb);
}
EXPORT_SYMBOL_GPL(mt76_rx);

void mt76_set_channel(struct mt76_dev *dev)
{
	struct ieee80211_hw *hw = dev->hw;
	struct cfg80211_chan_def *chandef = &hw->conf.chandef;
	struct mt76_channel_state *state;
	bool offchannel = hw->conf.flags & IEEE80211_CONF_OFFCHANNEL;

	if (dev->drv->update_survey)
		dev->drv->update_survey(dev);

	dev->chandef = *chandef;

	if (!offchannel)
		dev->main_chan = chandef->chan;

	if (chandef->chan != dev->main_chan) {
		state = mt76_channel_state(dev, chandef->chan);
		memset(state, 0, sizeof(*state));
	}
}
EXPORT_SYMBOL_GPL(mt76_set_channel);

int mt76_get_survey(struct ieee80211_hw *hw, int idx,
		    struct survey_info *survey)
{
	struct mt76_dev *dev = hw->priv;
	struct mt76_sband *sband;
	struct ieee80211_channel *chan;
	struct mt76_channel_state *state;
	int ret = 0;

	if (idx == 0 && dev->drv->update_survey)
		dev->drv->update_survey(dev);

	sband = &dev->sband_2g;
	if (idx >= sband->sband.n_channels) {
		idx -= sband->sband.n_channels;
		sband = &dev->sband_5g;
	}

	if (idx >= sband->sband.n_channels)
		return -ENOENT;

	chan = &sband->sband.channels[idx];
	state = mt76_channel_state(dev, chan);

	memset(survey, 0, sizeof(*survey));
	survey->channel = chan;
	survey->filled = SURVEY_INFO_TIME | SURVEY_INFO_TIME_BUSY;
	if (chan == dev->main_chan)
		survey->filled |= SURVEY_INFO_IN_USE;

	spin_lock_bh(&dev->cc_lock);
	survey->time = div_u64(state->cc_active, 1000);
	survey->time_busy = div_u64(state->cc_busy, 1000);
	spin_unlock_bh(&dev->cc_lock);

	return ret;
}
EXPORT_SYMBOL_GPL(mt76_get_survey);

void mt76_rx_complete(struct mt76_dev *dev, enum mt76_rxq_id q)
{
	struct sk_buff *skb;

	while ((skb = __skb_dequeue(&dev->rx_skb[q])) != NULL)
		ieee80211_rx_napi(dev->hw, NULL, skb, &dev->napi[q]);
}
